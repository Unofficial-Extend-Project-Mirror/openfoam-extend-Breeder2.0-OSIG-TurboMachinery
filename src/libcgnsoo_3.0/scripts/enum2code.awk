#!/usr/bin/awk

# usage: awk -f enum2code.awk <enum.dat >enum_table.h

function map(s) {
        nm=split(s,a,"_");
        w = "";
        for (iw in a ) {
                left =substr(a[iw],1,1);
                right=substr(a[iw],2);
                m=toupper(left) tolower(right);
                w = w m;
        }
        return w;
}
/\/\// { print; }
/[[:space:]]+(\w+)\,/ {
  nom=substr($1,1,length($1)-1);
  print "\t\t{ " nom ",\t\"" map(nom) "\" },";
}
