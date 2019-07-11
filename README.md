# Online Score System

## 概要

簡単なオンラインスコアシステムを構築するためのサーバー+クライアントです。
セキュリティに関しては全く無防備なので必ず各人のコーディングスタイルで必要部分を追加してください。

## サーバー側

### インストール

PHP の機能するディレクトリに配置するだけです。

### サーバー側API

#### Get メソッドで利用可能

|クエリパラメータ|説明|
|:--:|:--:|
|Reset|ランキングをリセットします。何も確認を取りません。|
|Format|出力形式を指定します。CSVまたはJSONがデフォルトで有効です。|

#### Post メソッドで利用可能

|クエリパラメータ|説明|
|:--:|:--:|
|Score|登録するスコア。PlayerName と同時に指定する必要があります。|
|PlayerName|登録するプレイヤー名。Score と同時に指定する必要があります。|