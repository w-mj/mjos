
class Class {
public:
	Class(int a) {
		this->a = a * a;
	}
	int a;
};

int main(void) {
	Class c(200);
	return c.a;
}
