--[[
  Quest 774: [Party] Bears the Chief
  Level Required: 33
  NPC Start: 29, NPC Complete: 29
  Rewards: EXP=53421, Gold=5803
]]

local fsm = require('fsm_engine')

local quest_774 = fsm:new({
    id = 774,
    name = "[Party] Bears the Chief",
    level_required = 33,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 774: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 774: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 774: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 139,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 774: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 774: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 774: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 774: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 774: Rewards given")
                self:give_rewards({
                    exp = 53421,
                    gold = 5803,
                })
            end,
            transitions = {},
        },
    },
})

return quest_774