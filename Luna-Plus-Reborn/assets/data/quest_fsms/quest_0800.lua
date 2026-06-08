--[[
  Quest 800: [Party] Seal the Cave
  Level Required: 67
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=920321, Gold=21542
]]

local fsm = require('fsm_engine')

local quest_800 = fsm:new({
    id = 800,
    name = "[Party] Seal the Cave",
    level_required = 67,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 800: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 800: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 800: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 103,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 800: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 800: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 800: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 800: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 800: Rewards given")
                self:give_rewards({
                    exp = 920321,
                    gold = 21542,
                })
            end,
            transitions = {},
        },
    },
})

return quest_800