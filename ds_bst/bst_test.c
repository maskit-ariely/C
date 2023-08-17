#include <stdio.h>	/* printf */
#include <string.h>	/* strcmp */
#include "bst.h"
#include "utilities.h"

int CompareInt(const void *data1, const void *data2)
{
	if(*(int*)data1 > *(int*)data2)
	{
		return 1;
	}
	else if(*(int*)data1 < *(int*)data2)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

int CompareStr(const void *data1, const void *data2)
{
	return strcmp((const char*)data1, (const char*)data2);
}


int Counter(void *data, void *parameter)
{
	++*(int*)(parameter);
	(void)data;
	
	return 0;
}


int PrintInt(void *data, void *parameter)
{
	printf("%d ", *(int*)data);
	(void)parameter;
	
	return 0;
}


int PrintStr(void *data, void *parameter)
{
	printf("%s ", (char*)data);
	(void)parameter;
	
	return 0;
}


int main()
{	
	/* test case 1 -  int */
	bst_t *management1 = NULL;
	int param1 = 0;
	int x1 = 3;
	int x2 = 5;
	int x3 = 2;
	int x4 = 9;
	int x5 = 4;
	int x6 = 6;
	int x7 = 1;
	int x8 = 100;
	
	/* test case 2 -  string */
	bst_t *management2 = NULL;
	int param2 = 0;
	char str1[5] = "this";
	char str2[3] = "is";
	char str3[3] = "my";
	char str4[6] = "check";
	char str5[4] = "for";
	char str6[7] = "binary";
	char str7[7] = "search";
	char str8[5] =  "tree";
	
	
	/********** TEST CASE 1 - INT **********/
	printf("********** TEST CASE 1 - INT **********\n\n");
	
	/***** BstCreate *****/
	printf("\n----- BstCreate -----\n\n");
	management1 = BstCreate(CompareInt);

	/* isempty, height, size */
	TESTS(1 == BstIsEmpty(management1));
	TESTS(0 == BstHeight(management1));
	TESTS(0 == BstSize(management1));
	
	TESTS(NULL == (int*)BstFind(management1, &x5));


	/***** BstInsert *****/
	printf("\n\n----- BstInsert -----\n\n");
	TESTS(0 == BstInsert(management1, &x1));
	TESTS(0 == BstInsert(management1, &x2));
	TESTS(0 == BstInsert(management1, &x3));
	
	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management1));
	TESTS(2 == BstHeight(management1));
	TESTS(3 == BstSize(management1));
	
	TESTS(0 == BstInsert(management1, &x4));
	TESTS(0 == BstInsert(management1, &x5));
	TESTS(0 == BstInsert(management1, &x6));
	TESTS(0 == BstInsert(management1, &x7));

	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management1));
	TESTS(4 == BstHeight(management1));
	TESTS(7 == BstSize(management1));
	
	/* print */
	printf("\n\n----- print IN_ORDER -----\n\n");
	BstForEach(management1, IN_ORDER, PrintInt, NULL);	


	/***** BstFind *****/
	printf("\n\n\n----- BstFind -----\n\n");
	TESTS(3 == *(int*)BstFind(management1, &x1));
	TESTS(4 == *(int*)BstFind(management1, &x5));
	TESTS(1 == *(int*)BstFind(management1, &x7));
	TESTS(NULL == BstFind(management1, &x8));	/* a number that wasn't inserted */


	/***** BstForEach *****/
	printf("\n\n----- BstForEach -----\n\n");
	TESTS(0 == BstForEach(management1, PRE_ORDER, Counter, &param1));	
	TESTS(7 == param1);
	param1 = 0;
	TESTS(0 == BstForEach(management1, IN_ORDER, Counter, &param1));	
	TESTS(7 == param1);
	param1 = 0;
	TESTS(0 == BstForEach(management1, POST_ORDER, Counter, &param1));	
	TESTS(7 == param1);
	param1 = 0;

	
	/***** BstRemove *****/
	printf("\n\n----- BstRemove -----\n\n");
	BstRemove(management1, &x6);
	BstRemove(management1, &x3);
	
	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management1));
	TESTS(3 == BstHeight(management1));
	TESTS(5 == BstSize(management1));
	
	BstRemove(management1, &x2);
	BstRemove(management1, &x7);
	BstRemove(management1, &x5);
	BstRemove(management1, &x4);
	
	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management1));
	TESTS(1 == BstHeight(management1));
	TESTS(1 == BstSize(management1));
	
	
	/***** BstForEach *****/
	printf("\n\n----- BstForEach -----\n\n");
	TESTS(0 == BstForEach(management1, PRE_ORDER, Counter, &param1));	
	TESTS(1 == param1);
	param1 = 0;
	TESTS(0 == BstForEach(management1, IN_ORDER, Counter, &param1));	
	TESTS(1 == param1);
	param1 = 0;
	TESTS(0 == BstForEach(management1, POST_ORDER, Counter, &param1));	
	TESTS(1 == param1);
	
	
	/***** BstInsert *****/
	printf("\n\n----- BstInsert -----\n\n");
	TESTS(0 == BstInsert(management1, &x1));
	TESTS(0 == BstInsert(management1, &x2));
	TESTS(0 == BstInsert(management1, &x3));
	
	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management1));
	TESTS(3 == BstHeight(management1));
	TESTS(4 == BstSize(management1));
	
	/* print */
	printf("\n\n----- print IN_ORDER -----\n\n");
	BstForEach(management1, IN_ORDER, PrintInt, NULL);	
	

	BstDestroy(management1);
	
	
	
	/********** TEST CASE 2 - STRING **********/
	printf("\n\n\n********** TEST CASE 2 - STRING **********\n\n");
	
	/***** BstCreate *****/
	printf("\n----- BstCreate -----\n\n");
	management2 = BstCreate(CompareStr);

	/* isempty, height, size */
	TESTS(1 == BstIsEmpty(management2));
	TESTS(0 == BstHeight(management2));
	TESTS(0 == BstSize(management2));
	
	TESTS(NULL == (int*)BstFind(management2, &str5));


	/***** BstInsert *****/
	printf("\n\n----- BstInsert -----\n\n");
	TESTS(0 == BstInsert(management2, &str1));
	TESTS(0 == BstInsert(management2, &str2));
	TESTS(0 == BstInsert(management2, &str3));
	
	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management2));
	TESTS(3 == BstHeight(management2));
	TESTS(3 == BstSize(management2));
	
	TESTS(0 == BstInsert(management2, &str4));
	TESTS(0 == BstInsert(management2, &str5));
	TESTS(0 == BstInsert(management2, &str6));
	TESTS(0 == BstInsert(management2, &str7));

	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management2));
	TESTS(4 == BstHeight(management2));
	TESTS(7 == BstSize(management2));
	
	/* print */
	printf("\n\n----- print IN_ORDER -----\n\n");
	BstForEach(management2, IN_ORDER, PrintStr, NULL);


	/***** BstFind *****/
	printf("\n\n----- BstFind -----\n\n");
	TESTS(0 == strcmp((char*)BstFind(management2, &str1), "this"));
	TESTS(0 == strcmp((char*)BstFind(management2, &str5), "for"));
	TESTS(0 == strcmp((char*)BstFind(management2, &str7), "search"));
	TESTS(NULL == BstFind(management2, &str8));	/* a number that wasn't inserted */


	/***** BstForEach *****/
	printf("\n\n----- BstForEach -----\n\n");
	TESTS(0 == BstForEach(management2, PRE_ORDER, Counter, &param2));	
	TESTS(7 == param2);
	param2 = 0;
	TESTS(0 == BstForEach(management2, IN_ORDER, Counter, &param2));	
	TESTS(7 == param2);
	param2 = 0;
	TESTS(0 == BstForEach(management2, POST_ORDER, Counter, &param2));	
	TESTS(7 == param2);
	param2 = 0;

	
	/***** BstRemove *****/
	printf("\n\n----- BstRemove -----\n\n");
	BstRemove(management2, &str6);
	BstRemove(management2, &str3);
	
	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management2));
	TESTS(3 == BstHeight(management2));
	TESTS(5 == BstSize(management2));
	
	BstRemove(management2, &str2);
	BstRemove(management2, &str7);
	BstRemove(management2, &str5);
	BstRemove(management2, &str4);
	
	/* find */
	TESTS(NULL == BstFind(management2, &str3));
	TESTS(NULL == BstFind(management2, &str5));
	
	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management2));
	TESTS(1 == BstHeight(management2));
	TESTS(1 == BstSize(management2));
	
	
	/***** BstForEach *****/
	printf("\n\n----- BstForEach -----\n\n");
	TESTS(0 == BstForEach(management2, PRE_ORDER, Counter, &param2));	
	TESTS(1 == param2);
	param2 = 0;
	TESTS(0 == BstForEach(management2, IN_ORDER, Counter, &param2));	
	TESTS(1 == param2);
	param2 = 0;
	TESTS(0 == BstForEach(management2, POST_ORDER, Counter, &param2));	
	TESTS(1 == param2);

	
	/***** BstInsert *****/
	printf("\n\n----- BstInsert -----\n\n");
	TESTS(0 == BstInsert(management2, &str1));
	TESTS(0 == BstInsert(management2, &str2));
	TESTS(0 == BstInsert(management2, &str3));
	
	/* isempty, height, size */
	TESTS(0 == BstIsEmpty(management2));
	TESTS(3 == BstHeight(management2));
	TESTS(4 == BstSize(management2));
	
	/* print */
	printf("\n\n----- print IN_ORDER -----\n\n");
	BstForEach(management2, IN_ORDER, PrintStr, NULL);

	printf("\n");


	BstDestroy(management2);
	

	return 0;
}
