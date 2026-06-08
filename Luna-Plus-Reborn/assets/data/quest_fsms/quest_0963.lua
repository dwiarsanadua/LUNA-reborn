--[[
  Quest 963: [Dungeon Quest]Evil Towers(Beginner)
  Level Required: 30
  NPC Start: 545, NPC Complete: 545
  Rewards: EXP=37033, Gold=2994
]]

local fsm = require('fsm_engine')

local quest_963 = fsm:new({
    id = 963,
    name = "[Dungeon Quest]Evil Towers(Beginner)",
    level_required = 30,
    npc_start = 545,
    npc_complete = 545,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 963: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 963: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 963: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 448,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 963: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 450,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 963: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 455,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 963: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 459,
                    count = 30,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 963: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 963: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 963: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 545,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 963: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 963: Rewards given")
                self:give_rewards({
                    exp = 37033,
                    gold = 2994,
                })
            end,
            transitions = {},
        },
    },
})

return quest_963