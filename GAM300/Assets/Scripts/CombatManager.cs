using System.Collections;
using BeanFactory;
using GlmSharp;

public class CombatManager : Script
{
    public static CombatManager instance;

    //ADD A GAMEOBJECT FOR EACH SOUND
    public float damagedShakeMag = 1;
    public float damagedShakeDur = 1;
    public float hitShakeMag = 1;
    public float hitShakeDur = 1;

    public float hitEffectDuration = 1f;

    public GameObject hitEffect;

    void Awake()
    {
        if (instance != null)
        {
            //Debug.LogError("More than one AudioManager in the scene.");
        }
        else
        {
            instance = this;
        }
    }

    public void SpawnHitEffect(Transform transform)
    {
        GameObject particles = Instantiate(hitEffect, transform.position, transform.rotation);
        StartCoroutine(DestroyParticles(hitEffectDuration/2f, particles));
    }

    IEnumerator DestroyParticles(float duration,GameObject gameObj)
    {
        float elapsed = 0f;
        while (elapsed < duration)
        {
            elapsed += Time.deltaTime;
            yield return null;
        }
        Destroy(gameObj);
    }
}