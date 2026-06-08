--[[
  Quest 974: [Dungeon Quest] Minion of the Wreck (Intermediate)
  Level Required: 60
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=190259, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_974 = fsm:new({
    id = 974,
    name = "[Dungeon Quest] Minion of the Wreck (Intermediate)",
    level_required = 60,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 974: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 974: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 974: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 700,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 974: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 701,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 974: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 702,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 974: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 703,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 974: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 974: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 974: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 974: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 974: Rewards given")
                self:give_rewards({
                    exp = 190259,
                    gold = 12128,
                    items = {
                        { item_id = 21000008, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_974