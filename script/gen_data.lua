header = {
    { name='size', type='uint16_t' },
    { name='type', type='uint16_t' },
    { name='sequence', type='sequence_t' }
}

session = {
    { name='session_id', type='session_id_t' }
}

login = {
    { name='user_id', type='char', length="16" }
}

resend = {
    { name='sequence_from', type='sequence_t' },
    { name='sequence_to', type='sequence_t' }
}

order_parameters = {
    { name='symbol', type='char', length='8' },
    { name='type', type='order_type_t' },
    { name='side', type='order_side_t' },
    { name='quantity', type='order_quantity_t' },
    { name='price', type='order_price_t' }
}

place_order = {
    { name='transaction_id', type='exchange_id_t' },
    { name='parameters', type='order_parameters' }
}

pull_order = {
    { name='transaction_id', type='exchange_id_t' }
}

order_rejected = {
    { name='transaction_id', type='exchange_id_t' },
    { name='parameters', type='order_parameters' }
}

order_placed = {
    { name='transaction_id', type='exchange_id_t' },
    { name='parameters', type='order_parameters' }
}

order_pulled = {
    { name='transaction_id', type='exchange_id_t' },
    { name='leaves', type='order_quantity_t' },
    { name='parameters', type='order_parameters' }
}

order_executed = {
    { name='transaction_id', type='exchange_id_t' },
    { name='exec_price', type='order_price_t' },
    { name='exec_quantity', type='order_quantity_t' },
    { name='leaves', type='order_quantity_t' },
    { name='parameters', type='order_parameters' }
}

names = {
    'session',
    'header',
    'login',
    'resend',
    'order_parameters',
    'order_rejected',
    'order_placed',
    'order_pulled',
    'order_executed',
    'place_order',
    'pull_order'
}
