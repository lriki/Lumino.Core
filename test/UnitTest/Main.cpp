
#include <stdio.h>
#include "gtest/gtest.h"

GTEST_API_ int main(int argc, char **argv)
{
	// Process クラスのテストスタブ
	if (strcmp(argv[argc - 1], "--proctest1") == 0)
	{
		printf("stdout");
		return 5;
	}
	else if (strcmp(argv[argc - 1], "--proctest2") == 0)
	{
		char str[256];
		scanf("%s", &str);
		return strlen(str);		// 文字数を返す
	}
	else if (strcmp(argv[argc - 1], "--proctest3") == 0)
	{
		char str[256];
		scanf("%s", &str);
		printf("[%s]", str);	// [ ] をつけて出力
		return 0;
	}


	printf("Running main()\n");
	
#ifdef _WIN32
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
	setlocale(LC_ALL, "");

#if 1	// 部分的にテストを実行したりする
	char* testArgs[] = {
		argv[0],
		//"--gtest_filter=Test_Text_EncodingDetector.*"
		"--gtest_filter=Test_Json_JsonReader.*"
	};
	argc = sizeof(testArgs) / sizeof(char*);
	testing::InitGoogleTest(&argc, (char**)testArgs);
#else
	testing::InitGoogleTest(&argc, argv);
#endif
	return RUN_ALL_TESTS();
}
