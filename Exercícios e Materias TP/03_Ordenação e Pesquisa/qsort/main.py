# coding=utf-8
from sys import exit as _exit


def quicksort(arr: list):
	if len(arr) <= 1:
		return arr
	else:
		pivot = arr[len(arr) // 2]
		left = [x for x in arr if x < pivot]
		middle = [x for x in arr if x == pivot]
		right = [x for x in arr if x > pivot]
		return quicksort(left, len(left)) + middle + quicksort(right, len(right))


if __name__ == "__main__":
	sample_array = [3, 6, 8, 10, 1, 2, 1, 6, 8, 5, 11, 5, 4]
	sorted_array = quicksort(sample_array)
	print("Sorted array:", sorted_array)
	_exit(0)
