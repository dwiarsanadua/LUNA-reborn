--[[
  Quest 962: Far Away Memories
  Level Required: 0
  NPC Start: 543, NPC Complete: 543
  Prerequisites: [961]
  Rewards: EXP=20095498, Gold=106426
]]

local fsm = require('fsm_engine')

local quest_962 = fsm:new({
    id = 962,
    name = "Far Away Memories",
    level_required = 0,
    npc_start = 543,
    npc_complete = 543,
    prerequisites = {961},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 962: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 543,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 962: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 962: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 326,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 962: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 345,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 962: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 543,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 962: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 962: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 543,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 962: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 962: Rewards given")
                self:give_rewards({
                    exp = 20095498,
                    gold = 106426,
                    items = {
                        { item_id = 21000010, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_962