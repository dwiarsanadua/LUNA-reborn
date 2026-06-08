--[[
  Quest 775: [Party] Golem's Head
  Level Required: 33
  NPC Start: 29, NPC Complete: 29
  Rewards: EXP=53421, Gold=5803
]]

local fsm = require('fsm_engine')

local quest_775 = fsm:new({
    id = 775,
    name = "[Party] Golem's Head",
    level_required = 33,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 775: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 775: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 775: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 140,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 775: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 775: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 775: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 775: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 775: Rewards given")
                self:give_rewards({
                    exp = 53421,
                    gold = 5803,
                })
            end,
            transitions = {},
        },
    },
})

return quest_775