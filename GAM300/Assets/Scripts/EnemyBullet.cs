using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class EnemyBullet : Script
{
    public bool destroyOnTrigger = true;

    public int damage = 1;

    void OnTriggerEnter(PhysicsComponent other)
    {
        if (!destroyOnTrigger)
            return;
        if (GetTag(other) != "Enemy" && GetTag(other) != "EnemyAttack" && GetTag(other) != "PlayerAttack")
        {
            Destroy(gameObject);
        }
    }

    public IEnumerator StartBullet(float duration,float waitTime, vec3 velocity)
    {
        yield return new WaitForSeconds(waitTime);

        Rigidbody rb = GetComponent<Rigidbody>();
        MeshRenderer mesh = GetComponent<MeshRenderer>();

        Material mat = mesh.material;

        float timer = duration;

        vec3 startScale = transform.localScale;

        vec4 color = mat.color;

        float emission = mat.emission;

        vec4 targetColor = color;

        targetColor.a = 0;
        while (timer > 0)
        {

            mat.color = vec4.Lerp(targetColor, color, timer / duration);
            mat.emission = glm.Lerp(0, emission, timer / duration);
            rb.linearVelocity = velocity * Time.deltaTime;
            transform.localScale = vec3.Lerp(startScale*2f, startScale, timer / duration);
            timer -= Time.deltaTime;
            yield return null;
        }
        SetEnabled(rb,false);
        SetEnabled(mesh, false);
        yield return new WaitForSeconds(RNG.Range(0,1f));
        Destroy(gameObject);
    }
}
