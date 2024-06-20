#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

mt19937_64 rng((long long) chrono::steady_clock::now().time_since_epoch().count());

template<class Info, class Tag>
struct Treap {
	long long data=0;
    long long priority;
	array<Treap*, 2> kids;
	int subtreeSize=1;
    bool rev;
    Info sum;
    Tag toProp;
    Treap() {
        kids={NULL, NULL};
        this->data = 0;
        this->rev=0;
        recalc(this);
        priority=rng();
    }
    Treap(long long data) {
        kids={NULL, NULL};
        this->data = data;
        this->rev=0;
        recalc(this);
        priority=rng();
    }
};

template<class Info, class Tag>
int size(Treap<Info,Tag> *me) {
	return me == NULL ? 0 : me->sum.size;
}

//pull
template<class Info, class Tag>
void recalc(Treap<Info,Tag> *me) {
	if (me==NULL) return;
	me->subtreeSize = 1;
	me->sum=Info(me->data);
    me->sum.apply(me->toProp);
	for (Treap<Info,Tag>* t:me->kids) {
        if (t != NULL){
            Info x=t->sum;
            x.apply(t->toProp);
            me->sum = me->sum+x;
        }
    }
}

//push
template<class Info, class Tag>
void prop(Treap<Info,Tag> *me) {
	if (me==NULL) return;
    if(me->rev){
        me->rev=0;
        swap(me->kids[0],me->kids[1]);
        for (Treap<Info,Tag> *t:me->kids) if (t != NULL) t->rev^=1;
    }
	if (me->toProp == Tag()) return;
	for (Treap<Info,Tag> *t:me->kids) if (t != NULL) t->toProp.apply(me->toProp);
	me->data=me->data+(me->toProp);
	me->toProp=Tag();
	recalc(me);
}

template<class Info, class Tag>
Treap<Info,Tag>* merge(Treap<Info,Tag> *l, Treap<Info,Tag> *r) {
	if (l==NULL) return r;
	if (r==NULL) return l;
    
	prop(l);
    prop(r);
	if (l->priority < r->priority) {
		l->kids[1]=merge(l->kids[1], r);
		recalc(l);
		return l;
	}
	else {
		r->kids[0]=merge(l, r->kids[0]);
		recalc(r);
		return r;
	}
}

template<class Info, class Tag>
array<Treap<Info,Tag>*, 2> split(Treap<Info,Tag> *me, int nInLeft) {
	if (me == NULL) return {NULL, NULL};
	prop(me);
	if (size(me->kids[0])>=nInLeft) {
		array<Treap<Info,Tag>*, 2> leftRes=split(me->kids[0], nInLeft);
		me->kids[0]=leftRes[1];
		recalc(me);
		return {leftRes[0], me};
	}
	else {
		nInLeft = nInLeft - size(me->kids[0]) - 1;
		array<Treap<Info,Tag>*, 2> rightRes = split(me->kids[1], nInLeft);
		me->kids[1] = rightRes[0];
		recalc(me);
		return {me, rightRes[1]};
	}
	return {NULL, NULL};
}


template<class Info, class Tag>
Treap<Info,Tag>* rangeAdd(Treap<Info,Tag>* t, int l, int r, Tag toAdd) {
	array<Treap<Info,Tag>*, 2> a=split(t, l), b=split(a[1], r-l+1);
	b[0]->toProp.apply(toAdd);
	return merge(a[0], merge(b[0], b[1]));
}

template<class Info, class Tag>
Treap<Info,Tag>* rangereverse(Treap<Info,Tag>* t, int l, int r) {
	array<Treap<Info,Tag>*, 2> a=split(t, l), b=split(a[1], r-l+1);
	b[0]->rev=1;
	return merge(a[0], merge(b[0], b[1]));
}

template<class Info, class Tag>
Info rangeQuery(Treap<Info,Tag>* t, int l, int r) {
	array<Treap<Info,Tag>*, 2> a=split(t, l), b=split(a[1], r-l+1);
	Info x=b[0]->sum;

	t= merge(a[0], merge(b[0], b[1]));
    return x;
}

struct Tag {
    ll mul=1;
    ll add=0;
    void apply(Tag t) {
        add=add*t.mul+t.add;
        mul*=t.mul;
    }
};

bool operator==(const Tag &a, const Tag &b) {
    return a.add==b.add && a.mul==b.mul;
}

struct Info {
    // ll val=0;
    ll size=0;
    ll ma=LLONG_MIN;
    // ll mi=LLONG_MAX;
    Info(){
        // val=0;
        size=0;
        // mi=0;
        ma=0;
    }
    Info(long long data){
        // val=data;
        size=1;
        // mi=data;
        ma=data;
    }
    void apply(Tag t) {
        // val=val*t.mul+t.add*size;
        // mi=mi*t.mul+t.add;
        ma=ma*t.mul+t.add;
    }
};

 
Info operator+(const Info &a, const Info &b) {
    Info c;
    // c.val=a.val+b.val;
    c.ma=max(a.ma,b.ma);
    // c.mi=min(a.mi,b.mi);
    // c.size=a.size+b.size;
    return c;
}
ll operator+(const ll &a, const Tag &b) {
    ll c;
    c=a*b.mul+b.add;
    return c;
}

struct pred {
    ll qu=0;
    bool apply(Info t) {
        return t.size<qu;
    }
};

template<class Info, class Tag>
ostream &operator<<(ostream &os, Treap<Info,Tag> *n) {
	if (!n) return os;
    prop(n);
	os << n->kids[0];
	os << n->data<<" ";
	os << n->kids[1];
    recalc(n);
	return os;
}


void solve(){

    ll n,m,k;
    cin>>n>>m>>k;
    vector<ll>v(n);
    for(int i=0;i<n;i++){
        cin>>v[i];
    }
    vector<Treap<Info,Tag>*>vt(m);
    for(int i=0;i<m;i++){
        vt[i]=NULL;
    }
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            Treap<Info,Tag> *p=new Treap<Info,Tag>(0);
            vt[j]=merge(vt[j],p);
        }
    }
    ll ans=0;
    for(int i=0;i<n;i++){
        for(int j=0;j<m;j++){
            vt[j]=rangeAdd(vt[j],0,i,{1,v[i]});
        }
        int ii=i%m;
        vt[ii]=rangeAdd(vt[ii],0,i,{1,-k});
        ans=max(ans,rangeQuery(vt[ii],0,i).ma);
    }
    cout<<ans<<"\n";

    // Treap<Info,Tag> *t;
    // t=NULL;

    // for(int i=0;i<n;i++){
    //     cin>>v[i];
    //     Treap<Info,Tag> *p;
    //     p= new Treap<Info,Tag>(v[i]);
    //     t=merge(t,p);
    // }
    // for(int i=0;i<m;i++){
    //     int tt=0;
    //     int l,r;
    //     cin>>tt;
    //     cin>>l>>r;
    //     l--;
    //     r--;
    //     if(tt==1){
    //         t=rangereverse(t,l,r);
    //     }
    //     else{
    //         cout<<rangeQuery(t,l,r).val<<"\n";
    //     }

    // }
    
    
}
int32_t main()
{
    ios::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);
    cout << setprecision(30) << fixed;
    int t=1;
    //cin>>t;
    for(int i=0;i<t;i++){
       solve();
    }
    return 0;
}