#!/usr/bin/env bash
rm ab_result

for i in {0..10}
do
    ab -n 100000 "http://vubuntu" >>ab_result
done


