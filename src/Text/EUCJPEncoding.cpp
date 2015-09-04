/*

	
	EUC-JP
		00-7F            - ASCII
		80-8D & 90-9F    - Control.
		A1-FE, A1-FE     - 2 byte JIS X 0208 (SJIS に含まれる)
		8E, A1-DF        - 2 byte 半角カタカナ
		8F, A1-FE, A1-FE - 3 byte JIX X 0212 (補助漢字。SJIS には含まれない。Unicodeには含まれる)


	ConvertTable_EUCJP_EXT のテーブルにアクセスするには、補助漢字の先行バイト 0x8F を取り除き、
	後の2バイトはそれぞれ -0x80 する。

*/