using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using BeanFactory;
using GlmSharp;


public class Inventory : Script
{
    public static Inventory instance;
    public ThirdPersonController thirdPersonController;

    public int healthPackCount;
    public int staminaPackCount;
    public int maxStatPackCount;

    public float healthPackHealValue = 5f;
    public int staminaPackValue = 50;

    void Awake()
    {
        //to fix issue where this reference is missing in the scene
        thirdPersonController = gameObject.GetComponent<ThirdPersonController>();

        if (instance != null)
        {
            //Debug.LogError("More than one AudioManager in the scene.");
        }
        else
        {
            instance = this;
        }
    }

    void Update()
    {
        thePacks();
       
    }

    void thePacks()
    {
        //HEALTH PACK - RESTORES HEALTH
        if (healthPackCount > 0 && Input.GetKey(KeyCode.D1))
        {
            healthPackCount -= 1;
            Console.WriteLine("Use Health Pack");
            AudioManager.instance.useItem.Play();
            thirdPersonController.HealHealth(healthPackHealValue);
            thirdPersonController.UpdatehealthBar();
        }

        //STAMINA PACK - RESTORE STAMINA
        if (staminaPackCount > 0 && Input.GetKey(KeyCode.D2))
        {
            staminaPackCount -= 1;
            Console.WriteLine("Use Stamina Pack");
            AudioManager.instance.useItem.Play();
            thirdPersonController.restoreStamina(staminaPackValue);
            thirdPersonController.UpdateStaminaBar();
        }

        //MAXSTAT PACK - RESTORE HEALTH AND STAMINA
        if (maxStatPackCount > 0 && Input.GetKey(KeyCode.D3))
        {
            maxStatPackCount -= 1;
            Console.WriteLine("Use MaxStat Pack");
            AudioManager.instance.useItem.Play();
            thirdPersonController.HealHealth(thirdPersonController.maxHealth);
            thirdPersonController.UpdatehealthBar();
        }
    }

}
