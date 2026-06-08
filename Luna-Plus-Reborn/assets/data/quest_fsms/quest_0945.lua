--[[
  Quest 945: [Party] George's Story
  Level Required: 0
  NPC Start: 541, NPC Complete: 541
  Prerequisites: [944]
  Rewards: EXP=13488379, Gold=101632
]]

local fsm = require('fsm_engine')

local quest_945 = fsm:new({
    id = 945,
    name = "[Party] George's Story",
    level_required = 0,
    npc_start = 541,
    npc_complete = 541,
    prerequisites = {944},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 945: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 945: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 945: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 337,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 945: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 945: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 945: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 541,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 945: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 945: Rewards given")
                self:give_rewards({
                    exp = 13488379,
                    gold = 101632,
                })
            end,
            transitions = {},
        },
    },
})

return quest_945