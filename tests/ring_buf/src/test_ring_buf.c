/*
 * Ring Buffer Module - Homework Test Skeleton
 *
 * test_fresh_state is provided as a worked example. Fill in the remaining
 * 7 ZTEST bodies according to TEST_SPEC.md. Stubs call ztest_test_skip()
 * so the binary builds and runs cleanly before each test is implemented.
 *
 * Run:
 *   west twister -T tests/ring_buf -p native_sim
 */

#include <zephyr/ztest.h>
#include <errno.h>

#include "ring_buf.h"

/*
 * Shared before hook: every suite reinitialises the ring buffer with a
 * capacity of 4 so tests start from a clean, known state. Capacity 4 is
 * enough to exercise FIFO order (push 1, 2, 3) and overflow (full at 4).
 */
static void before(void *f)
{
	ARG_UNUSED(f);
	rb_init(4);
}

/*
 * Note: rb_init(), rb_push(), rb_pop() and rb_peek() return 0 on success
 * (Zephyr convention), so success is checked with zassert_ok() (== 0),
 * not zassert_true(). rb_is_empty() and rb_is_full() return bool, so
 * zassert_true() is correct for those. rb_count() returns the count itself.
 */

/*
 * ============================================================================
 * Test Suite: ring_buf_init
 *
 * Initial state and re-initialization behaviour.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_init, NULL, NULL, before, NULL, NULL);

/* PROVIDED — study this test before writing the rest. */
ZTEST(ring_buf_init, test_fresh_state)
{
	zassert_true(rb_is_empty(), "Fresh buffer must be empty");
	zassert_equal(rb_count(), 0, "Fresh buffer count must be 0");
}

ZTEST(ring_buf_init, test_reinit_clears_state)
{
	/* TODO(l8-task1): Push a value, call rb_init(4) again, then
	 * verify the buffer is empty and count is 0.
	 * See TEST_SPEC.md "Suite ring_buf_init" #2.
	 */
	// push 99 to rb_buff, it should succeed as the buffer is empty now
	zassert_ok(rb_push(99), "Push 99 must succeed");
	// Initialize buffer again and set Capacity to 4
	zassert_ok(rb_init(4), "Init with Capacity of 4 must succeed");
	zassert_true(rb_is_empty(), "Re-initialised buffer must be empty");
	zassert_equal(rb_count(), 0, "Re-initialised buffer count must be 0");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_push_pop
 *
 * Single push/pop round-trip, FIFO order, full error path.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_push_pop, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_push_pop, test_single_push_pop)
{
	int pop_val = 0;
	/* TODO(l8-task1): rb_push(42), rb_pop(&v) -> v == 42, buffer empty after.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #1.
	 */

	// Verify #1
	zassert_ok(rb_push(42), "Push 42 must succeed");
	zassert_ok(rb_pop(&pop_val), "Pop must succeed");
	zassert_equal(pop_val, 42, "Popped value must match 42");
	zassert_true(rb_is_empty(), "After pop buffer must be empty");
}

ZTEST(ring_buf_push_pop, test_fifo_order)
{
	int pop_val = 0;
	/* TODO(l8-task1): rb_push(1), rb_push(2), rb_push(3); pop three times
	 * and verify the values come out as 1, 2, 3 in that order.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #2.
	 */

	// Verify #2
	zassert_ok(rb_push(1), "Push 1 must succeed");
	zassert_ok(rb_push(2), "Push 2 must succeed");
	zassert_ok(rb_push(3), "Push 3 must succeed");

	zassert_ok(rb_pop(&pop_val), "First pop must succeed");
	zassert_equal(pop_val, 1, "First popped value must match 1");
	zassert_ok(rb_pop(&pop_val), "Second pop must succeed");
	zassert_equal(pop_val, 2, "Second popped value must match 2");
	zassert_ok(rb_pop(&pop_val), "Third pop must succeed");
	zassert_equal(pop_val, 3, "Third popped value must match 3");

	zassert_true(rb_is_empty(), "After 3 pops buffer must be empty");
}

ZTEST(ring_buf_push_pop, test_push_full_returns_enospc)
{
	/* TODO(l8-task1): Fill the buffer to its capacity of 4, then push
	 * one more value -> -ENOSPC.
	 * See TEST_SPEC.md "Suite ring_buf_push_pop" #3.
	 */

	// Verify #3
	zassert_ok(rb_push(1), "Push 1 must succeed");
	zassert_ok(rb_push(2), "Push 2 must succeed");
	zassert_ok(rb_push(3), "Push 3 must succeed");
	zassert_ok(rb_push(4), "Push 4 must succeed");
	// Check rb_buff is now FULL with 4 values
	zassert_true(rb_is_full(), "After 4 pushes the buffer must be full");
	zassert_equal(rb_push(99), -ENOSPC,
		      "Push 99 into a full buffer must return -ENOSPC");
	// The rejected push must not have used a slot
	zassert_equal(rb_count(), 4, "Count must still be 4 after the rejected push");
}

/*
 * ============================================================================
 * Test Suite: ring_buf_boundaries
 *
 * Peek semantics and NULL-pointer boundary conditions.
 * ============================================================================
 */
ZTEST_SUITE(ring_buf_boundaries, NULL, NULL, before, NULL, NULL);

ZTEST(ring_buf_boundaries, test_peek_does_not_consume)
{
	/* TODO(l8-task1): rb_push(7); rb_peek(&v) -> v == 7; rb_peek(&v) again
	 * -> v == 7; rb_count() still == 1.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #1.
	 */
	int peek_val = 0;

	// Verify #4
	zassert_ok(rb_push(7), "Push 7 must succeed");

	zassert_ok(rb_peek(&peek_val), "First peek must succeed");
	zassert_equal(peek_val, 7, "First peek must return the value 7");

	peek_val = 0;	// clear it, so the second peek has to write 7 again
	zassert_ok(rb_peek(&peek_val), "Second peek must succeed");
	zassert_equal(peek_val, 7, "Second peek must return the value 7");

	zassert_equal(rb_count(), 1, "After one push and 2 peeks the count must still be 1");
}

ZTEST(ring_buf_boundaries, test_pop_null_returns_einval)
{
	/* TODO(l8-task1): rb_pop(NULL) -> -EINVAL.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #2.
	 */

	// Verify #5
	zassert_equal(rb_pop(NULL), -EINVAL,
		      "Popping into a NULL pointer must return -EINVAL");
}

ZTEST(ring_buf_boundaries, test_is_full_after_fill)
{
	/* TODO(l8-task1): push 4 values -> rb_is_full() == true, rb_count() == 4.
	 * See TEST_SPEC.md "Suite ring_buf_boundaries" #3.
	 */

	// Verify #6
	zassert_ok(rb_push(1), "Push 1 must succeed");
	zassert_ok(rb_push(2), "Push 2 must succeed");
	zassert_ok(rb_push(3), "Push 3 must succeed");
	zassert_ok(rb_push(4), "Push 4 must succeed");
	zassert_true(rb_is_full(), "After 4 pushes the buffer must be full");
	zassert_equal(rb_count(), 4, "After pushing 4 values the buffer count must be 4");
}

/*
NOTES :
 1)  Ran the command in WSL->Ubuntu 24.04
    $ west twister -T tests/ring_buf -p native_sim

 2) Ran the command:
    $ west twister -T tests/ring_buf -p native_sim \
       --coverage --coverage-tool gcovr \
        --coverage-basedir app/modules/ring_buf

3)
 And got the correct coverage report:
    Lines:     81.4%  (35/43)
    Functions: 100.0%  (7/7)
    Branches:  64.3%  (9/14)

 4) Looking at the RED lines in my coverage report and thinking: what test case(s) would be needed to cover those lines?
    Here are my answers to what i caould add to get 100% coverage:
    Add the 5 new tests described below to ring_buf.c to achieve 100% LINES, BRANCH, Functions Coverage

   Additional Test 1: test_init_zero_capacity_returns_einval
   zassert_equal(rb_init(0), -EINVAL, "rb_init(0) must return -EINVAL");

   Additional Test 2: test_init_over_max_capacity_returns_einval
   zassert_equal(rb_init(RING_BUF_MAX_CAPACITY + 1U), -EINVAL, "rb_init(MAX + 1) must return -EINVAL");
   zassert_ok(rb_init(RING_BUF_MAX_CAPACITY), "rb_init(MAX) must succeed");

   Additional Test 3: test_pop_empty_returns_enodata
   int v = 123;
    rb_init(4);
    zassert_equal(rb_pop(&v), -ENODATA, "Pop from empty buffer must return -ENODATA");
    zassert_equal(v, 123, "A failed pop must not change the output value");

   Additional Test 4: test_peek_null_returns_einval
   zassert_equal(rb_peek(NULL), -EINVAL, "rb_peek(NULL) must return -EINVAL");

   Additional Test 5: test_peek_empty_returns_enodata
   int v = 123;
   rb_init(4);
   zassert_equal(rb_peek(&v), -ENODATA, "Peek at empty buffer must return -ENODATA");
   zassert_equal(v, 123, "A failed peek must not change the output value");
   
*/