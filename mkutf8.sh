
for ANS in `ls -1 tests/pass/*.ans`; do 
		BASENAME=`basename $ANS`
		cat $ANS | iconv -f CP437 -t UTF8 > tests/pass/utf8/$BASENAME
		done
