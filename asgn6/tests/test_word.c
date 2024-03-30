#include "../word.h"
#include <check.h>
#include <stdint.h>
#include <stdlib.h>

#define TESTS "word_tests"
#define RESULT_FILE "results_word.tap"

START_TEST(test_wt_create) {
  WordTable *table = wt_create();
  ck_assert_msg(table, "failed to create Word Table");
  wt_delete(table);
}
END_TEST
START_TEST(test_word_create) {
  uint8_t syms[4] = {0x64, 0x65, 0x66, 0x67};
  Word *test = word_create(syms, 4);
  ck_assert_msg(test, "failed to create word");
  word_delete(test);
}
END_TEST

START_TEST(test_word_append_sym) {
  uint8_t syms[4] = {0x64, 0x65, 0x66, 0x67};
  Word *test = word_create(syms, 4);
  ck_assert_msg(test, "failed to create word");
  uint8_t syms_append[5] = {0x64, 0x65, 0x66, 0x67, 0x77};
  Word *test_append = word_append_sym(test, 0x77);
  ck_assert_msg(test_append->len == 5, "Incorrect length after append");
  for (int i = 0; i < test_append->len; i += 1) {
    ck_assert_msg(test_append->syms[i] == syms_append[i],
                  "Append failed, symbols don't match");
  }
  word_delete(test);
  word_delete(test_append);
}
END_TEST

START_TEST(test_word_delete) {
  uint8_t syms[4] = {0x64, 0x65, 0x66, 0x67};
  Word *test = word_create(syms, 4);
  ck_assert_msg(test, "failed to create word");
  word_delete(test);
  ck_assert_msg(test, "failed to delete word");
}
END_TEST

START_TEST(test_wt_reset) {
  WordTable *table = wt_create();
  uint8_t next = 2;
  for (int i = 1; i < 100; i += 1) {
    table[next] = word_append_sym(table[i], i);
    ck_assert_msg(table[next], "Unable to append to table at %d i is %d", next,
                  i);
    next += 1;
  }
  wt_reset(table);
  ck_assert_msg(table[1], "Reset deleted the whole table");
  table[2] = word_append_sym(table[1], 0x44);
  ck_assert_msg(table[2], "Unable to add new elements after reset");
  ck_assert_msg(table[2]->len - table[1]->len == 1,
                "Length not updated for new elements");

  wt_delete(table);
}
END_TEST

Suite *test_suite(void) {
  Suite *suite = suite_create("");

  TCase *tests = tcase_create(TESTS);
  tcase_add_test(tests, test_wt_create);
  tcase_add_test(tests, test_word_create);
  tcase_add_test(tests, test_word_append_sym);
  tcase_add_test(tests, test_wt_reset);
  tcase_add_test(tests, test_word_delete);
  suite_add_tcase(suite, tests);
  return suite;
}

int main(void) {
  Suite *s = test_suite();
  SRunner *runner = srunner_create(s);
  srunner_set_fork_status(runner, CK_FORK);
  srunner_set_tap(runner, RESULT_FILE);

  srunner_run_all(runner, CK_VERBOSE);
  int no_failed = srunner_ntests_failed(runner);
  srunner_free(runner);

  return (no_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
