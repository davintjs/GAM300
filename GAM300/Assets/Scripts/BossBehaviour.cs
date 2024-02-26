using System;
using System.Collections;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;

public class BossBehaviour : Script
{

    private enum BossState
    {
        Chase,
        BasicAttack,
        DashAttack,
        SlamAttack,
        UltimateAttack,
        UltimateCharge,
        ProjectileAttack,
    }

    BossState state = 0;

    public float chaseSpeed = 10f;

    public Animator animator;

    ThirdPersonController player;

    public Rigidbody rb;

    void Awake()
    {

    }

    void Start()
    {
        player = ThirdPersonController.instance;
    }

    void Update()
    {
        switch (state)
        {
            case BossState.Chase:
                ;
                break;
            case BossState.BasicAttack:
                break;
            case BossState.DashAttack:
                break;
            case BossState.SlamAttack:
                break;
            case BossState.UltimateAttack:
                break;
            case BossState.UltimateCharge:
                break;
            case BossState.ProjectileAttack:
                break;
            default:
                break;
        }
    }

    //Decision making is here
    void Chase()
    {
        animator.SetState("Run");
        float dist = vec3.Distance(transform.position,player.transform.position);
        vec3 dir = (player.transform.position - transform.position) / dist;

        rb.linearVelocity = dir * chaseSpeed * Time.deltaTime;
    }

/*    IEnumerator BasicAttack()
    {
      
    }*/

    IEnumerator DashAttack()
    {
        yield return null;
    }

    IEnumerator SlamAttack()
    {
        yield return null;
    }

    IEnumerator UltimateAttack()
    {
        yield return null;
    }

    IEnumerator ProjectileAttack()
    {
        yield return null;
    }
}