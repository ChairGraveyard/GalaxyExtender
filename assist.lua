echo('assist.lua executing....')

if not creature then
	return
end

lookAtTarget = creature:getLookAtTarget()

if not lookAtTarget then		
	return
end

obj = lookAtTarget:getObject()

if not obj then			
	return
end

creo = obj:asCreatureObject();

if not creo then				
	return
end

newLookAtTarget = creo:getLookAtTarget()

if not newLookAtTarget then								
	return				
end

newTargetObject = newLookAtTarget:getObject()

if not newTargetObject then						
	return
end

doCommand("target", newTargetObject)
