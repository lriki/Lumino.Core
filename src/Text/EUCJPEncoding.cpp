/*

	
	EUC-JP
		00-7F            - ASCII
		80-8D & 90-9F    - Control.
		A1-FE, A1-FE     - 2 byte JIS X 0208 (SJIS �Ɋ܂܂��)
		8E, A1-DF        - 2 byte ���p�J�^�J�i
		8F, A1-FE, A1-FE - 3 byte JIX X 0212 (�⏕�����BSJIS �ɂ͊܂܂�Ȃ��BUnicode�ɂ͊܂܂��)


	ConvertTable_EUCJP_EXT �̃e�[�u���ɃA�N�Z�X����ɂ́A�⏕�����̐�s�o�C�g 0x8F ����菜���A
	���2�o�C�g�͂��ꂼ�� -0x80 ����B

*/