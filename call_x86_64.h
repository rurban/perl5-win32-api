
/* size_t because it's definitely pointer-size. AFAIK no other int type is on both MSVC and gcc */
typedef union {
	size_t i;
	double d;
	float f;
} stackitem;

void Call_x64_real(FARPROC, size_t *, double *, stackitem *, size_t, size_t *, double *);

enum {
	available_registers = 4
};

void Call_asm(FARPROC ApiFunction, APIPARAM *params, int nparams, APIPARAM *retval, BOOL c_call)
{
	size_t nRegisters = 0, nStack = 0;
	size_t required_registers = 0, required_stack = 0;

	double float_registers[available_registers] = { 0., 0., 0., 0. };
	size_t int_registers[available_registers] = { 0, 0, 0, 0 };

	stackitem *stack = NULL;

	size_t iret;
	double dret;

	int i;

	required_registers = nparams > available_registers ? available_registers : nparams;
	required_stack = nparams > available_registers ? nparams - available_registers : 0;

	if (required_stack)
	{
		stack = malloc(required_stack * sizeof(*stack));
		memset(stack, 0, required_stack * sizeof(*stack));
	}

	for (i = 0; i < nparams; ++i)
	{
		if (i < available_registers)
		{
			/* First four arguments go in registers, either integer or floating point. */
			switch (params[i].t)
			{
				case T_NUMBER:
				case T_INTEGER:
				case T_CHAR:
					int_registers[i] = params[i].l;
					break;
				case T_POINTER:
				case T_STRUCTURE:
					int_registers[i] = params[i].p;
					break;
				case T_FLOAT:
					float_registers[i] = params[i].f;
					break;
				case T_DOUBLE:
					float_registers[i] = params[i].d;
					break;
			}
		}
		else
		{
			switch (params[i].t)
			{
				case T_NUMBER:
				case T_INTEGER:
					stack[i - available_registers].i = params[i].l;
					break;
				case T_POINTER:
				case T_STRUCTURE:
					stack[i - available_registers].i = params[i].p;
					break;
				case T_CHAR:
					stack[i - available_registers].i = params[i].c;
					break;
				case T_FLOAT:
					stack[i - available_registers].f = params[i].f;
					break;
				case T_DOUBLE:
					stack[i - available_registers].d = params[i].d;
					break;
			}
		}
	}

	Call_x64_real(ApiFunction, int_registers, float_registers, stack, required_stack, &iret, &dret);

	switch (retval->t)
	{
		case T_NUMBER:
		case T_INTEGER:
			retval->l = iret;
			break;
		case T_POINTER:
			retval->p = iret;
			break;
		case T_CHAR:
			retval->c = iret;
			break;
		case T_FLOAT:
			retval->f = dret;
			break;
		case T_DOUBLE:
			retval->d = dret;
			break;
	}
}


