echo "Cleaning Makefiles"
for i in $(find -name 'Makefile' -not -path "./external/imgui/examples/*");
do
    echo $i ; rm $i
done;
if [ -e "Iceberg.make" ] ; then
    echo "./Iceberg.make" && rm "Iceberg.make"
fi

echo "Deleting generated folders"
if [ -e "./bin" ] ; then
    echo "./bin" && rm -rf "./bin"
fi
if [ -e "./bin-int" ] ; then
    echo "./bin-int" && rm -rf "./bin-int"
fi