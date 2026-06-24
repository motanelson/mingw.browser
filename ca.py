import copy
print("\033[47;31m\n\ngive me the list file")
a=input().strip()
f1=open(a,"r")
b=f1.read()
f1.close()
c=b.split("\n")
counter=0
n="""
match var0:
"""
h="""
    case $1:
        var1=\"$2\"
"""
#j=copy.copy(g)
#print(j)

for z in range(0,len(c)):
    m=c[z].split(",")
    k=copy.copy(h)
    if len(m)>1:
        k=k.replace("$1",m[1])
        k=k.replace("$2",m[0])
        n=n+"\n"+copy.copy(k)
a=a.replace("lst","py")
f1=open(a,"w")
f1.write(n)
f1.close()
