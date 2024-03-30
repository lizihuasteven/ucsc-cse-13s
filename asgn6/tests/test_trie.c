#include "../trie.h"
#include <check.h>
#include <stdint.h>
#include <stdlib.h>

#define TESTS "trie_tests"
#define RESULT_FILE "results_trie.tap"

START_TEST(test_trie_create) {
  TrieNode *root = trie_create();
  ck_assert_msg(root, "failed to create trie");
  trie_delete(root);
  root = NULL;
}
END_TEST

START_TEST(test_trie_node_create) {
  TrieNode *root = trie_create();
  ck_assert_msg(root, "failed to create trie to test trie_node");
  root->children[(int)'a'] = trie_node_create(2);
  root->children[(int)'0'] = trie_node_create(3);
  root->children[(int)'0']->children[(int)'1'] = trie_node_create(4);
  ck_assert_msg(root->children[(int)'a'],
                "Failed to create node using trie_node_create");
  ck_assert_msg(root->children[(int)'0'],
                "Failed to create node using trie_node_create");
  ck_assert_msg(root->children[(int)'0']->children[(int)'1'],
                "Failed to create node using trie_node_create");
  ck_assert_msg(root->children[(int)'0']->code == 3,
                "Create node does not match given code");
  ck_assert_msg(root->children[(int)'a']->code == 2,
                "Create node does not match given code");
  ck_assert_msg(root->children[(int)'0']->children[(int)'1']->code == 4,
                "Create node does not match given code");
  trie_delete(root);
  root = NULL;
}
END_TEST

START_TEST(test_trie_node_delete) {
  TrieNode *root = trie_create();
  ck_assert_msg(root, "failed to create trie to test trie_node");
  root->children[(int)'a'] = trie_node_create(2);
  root->children[(int)'0'] = trie_node_create(3);
  root->children[(int)'0']->children[(int)'1'] = trie_node_create(4);
  ck_assert_msg(root->children[(int)'a'],
                "Failed to create node using trie_node_create");
  ck_assert_msg(root->children[(int)'0'],
                "Failed to create node using trie_node_create");
  ck_assert_msg(root->children[(int)'0']->children[(int)'1'],
                "Failed to create node using trie_node_create");

  TrieNode *child = root->children[(int)'0']->children[(int)'1'];
  trie_node_delete(child);
  ck_assert_msg(root->children[(int)'0'], "Deleted parent");
  root->children[(int)'0']->children[(int)'1'] = NULL;
  trie_delete(root);
  root = NULL;
}
END_TEST

START_TEST(test_trie_reset) {
  TrieNode *root = trie_create();
  ck_assert_msg(root, "failed to create trie to test trie_node");
  int child = 0;
  int code = 2;
  for (int i = 0; i < 256; i++) {
    int child = random() % 256;
    if (!root->children[child]) {
      root->children[child] = trie_node_create(code);
      ck_assert_msg(root->children[child],
                    "Failed to create node using trie_node_create");
      code += 1;
    }
  }
  trie_reset(root);
  ck_assert_msg(root, "Root should not be deleted");
  for (int i = 0; i < 256; i++) {
    ck_assert_msg(!root->children[i], "Children pointers not set to null");
  }
  trie_delete(root);
  root = NULL;
}
END_TEST

START_TEST(test_trie_step) {
  TrieNode *root = trie_create();
  ck_assert_msg(root, "failed to create trie to test trie_node");
  root->children[(int)'a'] = trie_node_create(2);
  root->children[(int)'0'] = trie_node_create(3);
  root->children[(int)'0']->children[(int)'1'] = trie_node_create(4);
  ck_assert_msg(root->children[(int)'a'],
                "Failed to create node using trie_node_create");
  ck_assert_msg(root->children[(int)'0'],
                "Failed to create node using trie_node_create");
  ck_assert_msg(root->children[(int)'0']->children[(int)'1'],
                "Failed to create node using trie_node_create");
  TrieNode *zero = trie_step(root, '0');
  ck_assert_msg(zero, "Failed to return child node");
  ck_assert_msg(zero->code == 3, "Incorrect child returned");
  TrieNode *one = trie_step(zero, '1');
  ck_assert_msg(one, "Failed to return child node");
  ck_assert_msg(one->code == 4, "Incorrect child returned");
  trie_delete(root);
  root = NULL;
}
END_TEST

Suite *test_suite(void) {
  Suite *suite = suite_create("");

  TCase *tests = tcase_create(TESTS);
  tcase_add_test(tests, test_trie_create);
  tcase_add_test(tests, test_trie_node_create);
  tcase_add_test(tests, test_trie_node_delete);
  tcase_add_test(tests, test_trie_step);
  tcase_add_test(tests, test_trie_reset);
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
