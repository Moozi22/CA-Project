

#include "pipeline.h"

#include <stdio.h>
#include <stdlib.h>

static int tests_run = 0;
static int tests_pass = 0;

#define CHECK(cond, msg)                  \
    do                                    \
    {                                     \
        tests_run++;                      \
        if (cond)                         \
        {                                 \
            tests_pass++;                 \
            printf("  PASS : %s\n", msg); \
        }                                 \
        else                              \
        {                                 \
            printf("  FAIL : %s\n", msg); \
        }                                 \
    } while (0)

static int write_program(const char *path, const char *program)
{
    FILE *fp = fopen(path, "w");
    if (!fp)
    {
        printf("  FAIL : unable to create %s\n", path);
        return 0;
    }

    fputs(program, fp);
    fclose(fp);
    return 1;
}

static void run_program_and_cleanup(const char *path)
{
    initMemory();
    initRegisters();

    int loaded = loadProgram(path);
    CHECK(loaded > 0, "Program loaded successfully");

    if (loaded > 0)
    {
        initPipeline(loaded);
        runPipeline();
    }

    remove(path);
}

static void test_data_dependency_chain(void)
{
    printf("\n--- Test 1: Data dependency chain ---\n");

    const char *path = "hazard_data_dependency.txt";
    const char *program =
        "MOVI R1 5\n"
        "ADD R1 R1\n"
        "ADD R2 R1\n";

    if (!write_program(path, program))
        return;

    run_program_and_cleanup(path);

    CHECK(getRegister(1) == 10, "R1 = 10 after dependent ADD chain");
    CHECK(getRegister(2) == 10, "R2 = 10 after dependent ADD R2 R1");
}

static void test_branch_flush(void)
{
    printf("\n--- Test 2: Branch flush / control hazard ---\n");

    const char *path = "hazard_branch_flush.txt";
    const char *program =
        "MOVI R1 0\n"
        "BEQZ R1 1\n"
        "MOVI R2 7\n"
        "MOVI R3 9\n";

    if (!write_program(path, program))
        return;

    run_program_and_cleanup(path);

    CHECK(getRegister(2) == 0, "R2 remains 0 because the flushed instruction did not execute");
    CHECK(getRegister(3) == 9, "R3 = 9 from the branch target");
    CHECK(completedInstructions == 3, "Only 3 instructions complete after branch flush");
}

int main(void)
{
    printf("=====================================================\n");
    printf("  PACKAGE 3  -  Pipeline Hazard Tests\n");
    printf("=====================================================\n");

    test_data_dependency_chain();
    test_branch_flush();

    printf("\n=====================================================\n");
    printf("  Results: %d / %d tests passed\n", tests_pass, tests_run);
    printf("=====================================================\n");

    return (tests_pass == tests_run) ? 0 : 1;
}