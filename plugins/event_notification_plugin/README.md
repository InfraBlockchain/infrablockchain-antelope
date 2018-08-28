# Event Notification Plugin (beta)

Based on `eosio::http_plugin` and `websocketpp`, this plugin registeres WebSocket message handlers for plain and TLS socket.
Through the HTTP and HTTPS ports listened via eosio::http_plugin, WebSocket clients which want to be notified events are subscribed.

## Usage Recommendation
This plugin is targeted to help to remove the sleep-based polling to check the irreversibility of transactions sent by DApps.
So it's recommended to use only for the transaction history access purpose for DApps.

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
  "name": "tx_irreversibility",
  "params": {
    "tx_id": "a4f2bfe30205cf8805aa17014759152414bc6db6879b9de465fefe91cd118db5"
  }
}
```

If the transaction is already irreversible, Yosemite node sends the response immediately.
But if not, when the block which includes the tranction above becomes irreversible, Yosemite node sends the response.
```json
{
  "name": "tx_irreversibility",
  "response": {
    "tx_id": "a4f2bfe30205cf8805aa17014759152414bc6db6879b9de465fefe91cd118db5"
  }
}
```

Note that this processing is done on WebSocket. The response for the request is not synchronous.
