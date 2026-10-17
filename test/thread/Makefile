create: 01create_test.c
	-gcc $^ -o $@
	-./$@
	-rm $@

sum: 02sum_test.c
	-gcc -Wall $^ -o $@ -g
# 	-./$@
# 	-rm $@

race_condition: 04_race_condition.c
	-$(CC) $^ -o $@
	-./$@
	-rm $@

mqueue: 06_mqueue.c
	-$(CC) $^ -o $@
	-./$@
	-rm $@