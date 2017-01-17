function echo(text) 
	c_echo(tostring(text)) 
end

function getHealth()
 	return tostring(getPlayerAttribute(0))
end

function getAction()
	return tostring(getPlayerAttribute(3))
end

echo(12345)
echo("Reloaded client_lua")