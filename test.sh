R --slave -f 'test.R' --args 10000 2> err | ./moments
cat err
