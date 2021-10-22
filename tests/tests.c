// Artifacts
#include <assert.h>
#include <CUnit/TestRun.h>
#include <CUnit/CUnit.h>
#include <CUnit/Automated.h>

// Includes
#include "../src/mya.h"

// TODO: This won't work for now since your functions are void. To unittests those functions the data, you could make mocks or return the error status.
// TODO: However that isn't something for me to decide. You're able to use unittests / test-suites now in your projects :)
/*------------------ Test Cases ------------------*/
void validate_too_short_username()
{
    // validate_username(""); // < 2
    // CU_ASSERT()...
}

void validate_too_long_username()
{
    // validate_username("TestUsernameWhichExceedsTheMaxLimits"); // > 16
    // CU_ASSERT()...
}

CU_TestInfo testcases[] = {
    {"CheckShortValidationOnUsername", validate_too_short_username},
    {"CheckLongValidationOnUsername", validate_too_long_username},
    CU_TEST_INFO_NULL
};

/**---------- test suites ------------------*/
int suite_success_init(void) { return 0; }
int suite_success_clean(void) { return 0; }

CU_SuiteInfo suites[] = {
        {"MYA_Tests", suite_success_init, suite_success_clean, NULL, NULL, testcases},
        CU_SUITE_INFO_NULL
};

void AddTests(void)
{

    CU_initialize_registry();
    assert(NULL != CU_get_registry());
    assert(!CU_is_test_running());

    if(CUE_SUCCESS != CU_register_suites(suites)){
            fprintf(stderr, "Register suites failed - %s ", CU_get_error_msg());
            exit(1);
    }
}

int main( int argc, char *argv[] )
{
    CU_cleanup_registry();

   if(CU_initialize_registry()){
            fprintf(stderr, " Initialization of Test Registry failed. ");
            exit(1);
    }else{
            AddTests();
            CU_set_output_filename("TestMax");
            CU_list_tests_to_file();
            CU_automated_run_tests();
            CU_cleanup_registry();
    }
    return 0;
}