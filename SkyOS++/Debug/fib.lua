-- very inefficient fibonacci function

function fib(n)
	if n<2 then
		return n
	else
		return fib(n-1)+fib(n-2)
	end
end

function test(x)
	write("fibonacci(", x,") : ", fib(x),"\n")
end


test(20)