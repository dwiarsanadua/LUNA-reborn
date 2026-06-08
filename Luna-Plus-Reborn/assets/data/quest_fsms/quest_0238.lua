--[[
  Quest 238: [Repeatable] Threatdown: Red Bears
  Level Required: 26
  NPC Start: 29, NPC Complete: 29
  Prerequisites: [237]
  Rewards: EXP=16826, Gold=3000
]]

local fsm = require('fsm_engine')

local quest_238 = fsm:new({
    id = 238,
    name = "[Repeatable] Threatdown: Red Bears",
    level_required = 26,
    npc_start = 29,
    npc_complete = 29,
    prerequisites = {237},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 238: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 238: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 238: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 50,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 238: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 238: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 238: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 29,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 238: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 238: Rewards given")
                self:give_rewards({
                    exp = 16826,
                    gold = 3000,
                })
            end,
            transitions = {},
        },
    },
})

return quest_238