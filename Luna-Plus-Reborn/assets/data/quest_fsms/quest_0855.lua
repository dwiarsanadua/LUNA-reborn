--[[
  Quest 855: Magapui Tears
  Level Required: 88
  NPC Start: 128, NPC Complete: 128
  Prerequisites: [854]
  Rewards: EXP=4107820, Gold=57826
]]

local fsm = require('fsm_engine')

local quest_855 = fsm:new({
    id = 855,
    name = "Magapui Tears",
    level_required = 88,
    npc_start = 128,
    npc_complete = 128,
    prerequisites = {854},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 855: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 855: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 855: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 283,
                    count = 100,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 855: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 855: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 855: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 128,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 855: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 855: Rewards given")
                self:give_rewards({
                    exp = 4107820,
                    gold = 57826,
                    items = {
                        { item_id = 30000686, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_855