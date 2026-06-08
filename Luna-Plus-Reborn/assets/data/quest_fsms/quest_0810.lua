--[[
  Quest 810: [Party] In the Deep Caves
  Level Required: 71
  NPC Start: 117, NPC Complete: 117
  Rewards: EXP=1237080, Gold=23174
]]

local fsm = require('fsm_engine')

local quest_810 = fsm:new({
    id = 810,
    name = "[Party] In the Deep Caves",
    level_required = 71,
    npc_start = 117,
    npc_complete = 117,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 810: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 810: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 810: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 110,
                    count = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 810: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 810: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 810: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 117,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 810: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 810: Rewards given")
                self:give_rewards({
                    exp = 1237080,
                    gold = 23174,
                })
            end,
            transitions = {},
        },
    },
})

return quest_810