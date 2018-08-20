YOSEMITE Public Key, Private Key, Signature Format
---

* YOSEMITE blockchain supports secp256k1 elliptic curve cryptography as default. (same as Bitcoin and EOS)
* Public/Private keys generated from the EOS blockchain software are supported on YOSEMITE chain.
* YOSEMITE blockchain supports R1 elliptic curve (secp256r1) cryptography also. secp256r1 is supported in widely used devices such as iPhone.


## YOSEMITE Keys

```bash
$YOSEMITE_CLYOS create key --to-console
Private key: YPV_5Jx4aC6fGkv5eaoYfKobibRGruV8xpgWWNXpojSNghJDehwrkhJ
Public key: YOS7Te8Jd4fF6HbTJH2BUYADBucb2yYYSRh3mVv4hgjkAJL15rzjF
```

#### YOSEMITE Public Key (secp256k1)
* key prefix of base58 public key string : "YOS"
* public key string format : "YOS" + base58 representation of 33 bytes secp256k1 public key data

#### YOSEMITE Private Key (secp256k1)
* key prefix of base58 public key string : "YPV"
* private key string format : "YPV_" + base58 representation of 256 bit secp256k1 private key secret in WIP format (https://en.bitcoin.it/wiki/Wallet_import_format)

#### YOSEMITE Signature (secp256k1)
* prefix of base58 signature string : "YSG_K1_"
* signature strubg format : "YSG_K1_" + base58 representation of 72 bytes secp256k1 signature data


## R1 Elliptic Curve Keys (secp256r1(=prime256v1))

```bash
$YOSEMITE_CLI create key --r1 --to-console
Private key: PVT_R1_C6ShjLAkRV7ULfzQQfYTWjjDL7HBUhGs7aXStgpWjkKLrTmdC
Public key: PUB_R1_6X2tkauA9gH3j2AujQCM79FR83jTf9MiRhZhubNmpt7146AWhu
```

#### secp256r1 Public Key
* key prefix of base58 public key string : "PUB_R1_"
* public key string format : "PUB_R1_" + base58 representation of 33 bytes secp256r1 public key data

#### secp256r1 Private Key
* key prefix of base58 public key string : "PVY_R1_"
* private key string format : "PVY_R1_" + base58 representation of 256 bit secp256r1 private key secret

#### secp256r1 Signature
* prefix of base58 signature string : "SIG_R1_"
* signature strubg format : "YSG_K1_" + base58 representation of 72 bytes secp256k1 signature data


## EOS Keys

```bash
eos/cleos create key
Private key: 5KLLX7piHozgByhonDC8Y8ypLXVx5AExmQqjb14o89DW34FUaGC
Public key: EOS7M4hiQnu83M5S5NM7Vx4JE57dVB3fxkFAckeMhLf7xpWW1wCdF
```

#### EOS Public Key (secp256k1)
* key prefix of base58 public key string : "EOS"
* public key string format : "EOS" + base58 representation of 33 bytes secp256k1 public key data

#### EOS Private Key (secp256k1)
* no key prefix of public key string, base public key
* private key string format : base58 representation of 256 bit secp256k1 private key secret in WIP format (https://en.bitcoin.it/wiki/Wallet_import_format)

#### EOS Signature (secp256k1)
* prefix of base58 signature string : "SIG_K1_"
* signature strubg format : "SIG_K1_" + base58 representation of 72 bytes secp256k1 signature data


