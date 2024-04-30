#!/bin/bash

echo "Enter the lecture id >> "
read lecture_id

echo "Enter the number of problems >> "
read prob_count

date=$(date +"%Y/%m/%d")

if [ ! -d "$lecture_id" ]; then
    mkdir "$lecture_id"
fi

cd "$lecture_id"
touch "$lecture_id.txt"

contents="""
科目名: コンパイラ
課題: $lecture_id
番号: 37022465
氏名: 高橋 由
作成: $date
"""

for i in $(seq 1 $prob_count);
do
problem="""
----------

$lecture_id.$i

1. 行ったこと


2. 結果
```

```

3. 感想

"""
contents="$contents$problem"
done

conclude="""
----------

[課題 $lecture_id の感想]
"""
contents="$contents$conclude"

echo "$contents" > "$lecture_id.txt"

echo "Lecture document created with the name: $lecture_id.txt"
