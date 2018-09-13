# Event Notification Plugin (beta)

Based on `eosio::http_plugin` and `websocketpp`, this plugin registers WebSocket message handlers for each type of sockets.
Through the HTTP and HTTPS ports listened via `eosio::http_plugin`, WebSocket clients which want to be notified events are subscribed.

## Usage Recommendation
It is targeted to help to remove the sleep-based polling to check the irreversibility of transactions sent by DApps.
So it's recommended to use only for the transaction history access purpose for DApps.

If you want to use this plugin, you must add to config.ini like below.

> plugin = yosemite::event_notification_plugin

## WebSocket URLs
ws://${http_plugin@http-server-address}/v1/event_notification

wss://${http_plugin@https-server-address}/v1/event_notification

## Operations
### subscribe
Currently, beta, this is just dummy operation.

### unsubscribe
Currently, beta, this is just dummy operation.

### tx_irreversibility
If a client registers the transaction id which want to check its irreversibility, it sends the request below.
```json
{
  "req_id": "1",
  "name": "tx_irreversibility",
  "tx_id": "a4f2bfe30205cf8805aa17014759152414bc6db6879b9de465fefe91cd118db5",
  "dropped": false
}
```

If the transaction is already irreversible, Yosemite node sends the corresponding response immediately.
But if not, when the block which includes the tranction above becomes irreversible, Yosemite node sends the response.
```json
{
  "req_id": "1",
  "name": "tx_irreversibility",
  "tx_id": "a4f2bfe30205cf8805aa17014759152414bc6db6879b9de465fefe91cd118db5",
  "dropped": false
}
```

If the transaction is expired, Yosemite node sends the corresponding response with `dropped` flag as true.
```json
{
  "req_id": "2",
  "name": "tx_irreversibility",
  "tx_id": "c0a9e9bac509f0dd37c93b49ba02947ed33c6cb831f5e7c6cd9e7c0b765ffdb6",
  "dropped": true
}
```

Note that all the processings are done on WebSocket, which means the response is not synchronous to the request.
