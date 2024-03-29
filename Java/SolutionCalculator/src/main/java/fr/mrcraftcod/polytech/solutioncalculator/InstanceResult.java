package fr.mrcraftcod.polytech.solutioncalculator;

class InstanceResult
{
	private Instance instance; 
	private int scoreC;
	private int scorePython;

	public InstanceResult(Instance instance, int scoreC, int scorePython)
	{
		this.instance = instance;
		this.scoreC = scoreC;
		this.scorePython = scorePython;
	}
	
	public boolean isCBetter()
	{
		return scoreC <= scorePython;
	}
	
	public boolean isPythonBetter()
	{
		return scoreC > scorePython;
	}
	
	@Override
	public String toString()
	{
		return "InstanceResult{" + "instance=" + instance.getSource().toFile().getName() + ", scoreC=" + scoreC + ", scorePython=" + scorePython + '}';
	}
}
