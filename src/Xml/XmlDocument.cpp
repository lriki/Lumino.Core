﻿/*
「Extensible Markup Language (XML) 1.1 (第二版)」
http://www.eplusx.net/translation/W3C/REC-xml11-20060816/#NT-elementdecl

目次 Table of Contents

1 導入
    1.1 始まりと目標
    1.2 専門用語
    1.3 XML 1.1での変更点とその原理的説明
2 文書
    2.1 整形式のXML文書
    2.2 文字 Characters
    2.3 一般的な構文構成子
    2.4 文字データとマークアップ
    2.5 コメント
    2.6 処理命令
    2.7 CDATAセクション
    2.8 プロローグと文書型宣言
    2.9 スタンドアローン文書宣言
    2.10 ホワイトスペースの取り扱い
    2.11 行末の取り扱い
    2.12 言語識別
    2.13 正規化検査
3 論理構造
    3.1 開始タグ、終了タグ、空要素タグ
    3.2 要素型宣言							→ <!ELEMENT ...>
        3.2.1 要素内容						→ <!ELEMENT ...> の children
        3.2.2 混合内容						→ <!ELEMENT ...> の Mixed (#PCDATA)
    3.3 属性リスト宣言						→ <!ATTLIST ...>
        3.3.1 属性型						→ <!ATTLIST ...>
        3.3.2 属性デフォルト				→ 		#REQUIRED #IMPLIED #FIXED
        3.3.3 属性値正規化					→		CRLF を LF に直したりする。
    3.4 条件付きセクション					→		http://www.atmarkit.co.jp/aig/01xml/conditional.html
4 物理構造
    4.1 文字参照と実体参照					→ '&#'、&Name
    4.2 実体宣言							→ <!ENTITY ...>
        4.2.1 内部実体						→ <!ENTITY ...>
        4.2.2 外部実体						→ <!ENTITY SYSTEM>
    4.3 解析対象実体
        4.3.1 テキスト宣言					→ '<?xml' VersionInfo? EncodingDecl S? '?>'
        4.3.2 整形式の解析対象実体
        4.3.3 実体で使われる文字エンコーディング	→ <?xml encoding='UTF-8'?>
        4.3.4 実体のバージョン情報
    4.4 XMLプロセッサによる実体と参照の扱い			→ 以下、ENTITY の詳細仕様
        4.4.1 認識されない
        4.4.2 インクルードされる
        4.4.3 妥当性を検証する場合インクルードされる
        4.4.4 許されない
        4.4.5 リテラルの中でインクルードされる
        4.4.6 通知する
        4.4.7 バイパスされる
        4.4.8 パラメータ実体としてインクルードされる
        4.4.9 エラー
    4.5 実体の置換テキストの構築
    4.6 定義済み実体						→ amp、lt、gt、apos、quot
    4.7 記法宣言							→ <!NOTATION
    4.8 文書実体
5 適合性
    5.1 妥当性を検証するプロセッサとしないプロセッサ
    5.2 XMLプロセッサの使用
6 仕様書の記法
*/
