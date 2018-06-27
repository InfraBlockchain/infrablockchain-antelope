# Abstract
* From yx.token operations or account creation operation, fee is gathered to yx.feedist account.
* yx.feedist contract also exists and it's main role is distribution of gathered fee to the block producers.

# To test yx.feedist contract
* Assume that yx.token contract is prepared.

## deploy yx.feedist contract
`cleos set contract yx.feedist build/contracts/yx.feedist -p yx.feedist`

## call startfeedist operation
* To schedule dofeedist operation as deferred transaction
* [Temporary] for test convenience, the paramater of startfeedist is not days but seconds.
```
cleos push action yx.feedist startfeedist '{"days":10}' -p yx.feedist
```