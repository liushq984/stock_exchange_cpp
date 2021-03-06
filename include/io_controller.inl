using boost::asio::ip::tcp;

struct io_session : public std::enable_shared_from_this< io_session >
{
    io_session( io_controller&, tcp::socket );

    ~io_session();

    template< typename H >
    void read( H handler );

    void write( const io_event& );

    session_id_t id();

    void operator()( const login& );

    void operator()( const resend& );

    io_controller& ioc_;
    session_id_t id_;
    io_event event_;

    tcp::socket socket_;
};

io_session::io_session( io_controller& ioc, tcp::socket socket ) :
    ioc_( ioc ),
    id_( ioc.alloc_id() ),
    socket_( std::move( socket ) )
{
    session s;
    s.session_id_ = id_;
    event_.pack( s );
}

io_session::~io_session()
{
    ioc_.closed( id_ );
}

template< typename H >
void io_session::read( H handler )
{
    auto self( shared_from_this() );
    boost::asio::async_read(
        socket_,
        boost::asio::buffer( event_.header_buffer(), buffer_size< header >() ),
        boost::asio::transfer_exactly( buffer_size< header >() ),
        [ this, self, handler ]( boost::system::error_code ec, std::size_t length )
        {
            if( !ec )
            {
                header hdr;
                event_.unpack( hdr );

                boost::asio::async_read(
                    socket_,
                    boost::asio::buffer( event_.payload_buffer(), hdr.size_ ),
                    boost::asio::transfer_exactly( hdr.size_ ),
                    [ this, self, handler ]( boost::system::error_code ec, std::size_t length )
                    {
                        if( !ec )
                        {
                            handler( event_ );
                            read( handler );
                        }
                        else
                        {
                            std::cerr << "io_session " << id_ << " read error - " << ec.message() << std::endl;
                        }
                    } );
            }
            else
            {
                std::cerr << "io_session " << id_ << " read error - " << ec.message() << std::endl;
            }
        } );
}

inline void io_session::write( const io_event& e )
{
    boost::asio::write(
        socket_,
        boost::asio::buffer( e.header_buffer(), e.size() - buffer_size< session >() ) );
}

inline session_id_t io_session::id()
{
    return id_;
}

inline void io_session::operator()( const login& l )
{
}

inline void io_session::operator()( const resend& r )
{
    io_journal ioj( "out", true );
    ioj.read( id_, r.sequence_from_, r.sequence_to_, [ this ]( const io_event& e ) {
        write( e );
    } );
}

template< typename H >
void io_controller::accept( int port, H handler )
{
    std::function< void( tcp::acceptor&, tcp::socket& ) > accept_again =
        [&]( tcp::acceptor& acceptor, tcp::socket& socket )
        {
            acceptor.async_accept( socket,
                [&]( boost::system::error_code ec )
                {
                    if( !ec )
                    {
                        auto sess = std::make_shared< io_session >( *this, std::move( socket ) );
                        sess->read( handler );
                        sessions_[ sess->id() ] = sess.get();
                        accept_again( acceptor, socket );
                    }
                } );
        };

    tcp::acceptor acceptor( io_, tcp::endpoint( tcp::v4(), port ) );
    tcp::socket socket( io_ );
    accept_again( acceptor, socket );
    io_.run();
}

template< typename H >
void io_controller::connect( int port, H handler )
{
    tcp::socket socket( io_ );
    tcp::resolver resolver( io_ );
    boost::asio::connect( socket, resolver.resolve( { "localhost", "14002" } ) ); 
    auto sess = std::make_shared< io_session >( *this, std::move( socket ) );
    sess->read( handler );
    sessions_[ sess->id() ] = sess.get();
    io_.run();
}

inline void io_controller::write( const io_event& e )
{
    session s;
    e.unpack( s );

    io_.dispatch( [ this, s, e ]()
    {
        auto it = sessions_.find( s.session_id_ );
        if( it != sessions_.end() ) {
            it->second->write( e );
        } else {
            std::cerr << "io_controller write: unknown session_id " << s.session_id_ << std::endl;
        }
    });
}

inline void io_controller::closed( session_id_t id )
{
    sessions_.erase( id );
}

inline session_id_t io_controller::alloc_id()
{
    static session_id_t id = 0;
    return id++;
}
