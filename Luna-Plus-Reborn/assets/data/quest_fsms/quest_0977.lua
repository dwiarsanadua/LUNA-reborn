--[[
  Quest 977: [Dungeon Quest] Minion of the Wreck (Advanced)
  Level Required: 60
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=190259, Gold=12128
]]

local fsm = require('fsm_engine')

local quest_977 = fsm:new({
    id = 977,
    name = "[Dungeon Quest] Minion of the Wreck (Advanced)",
    level_required = 60,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 977: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 977: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 977: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 704,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 977: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 705,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 977: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 706,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 977: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 707,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 977: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 977: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 977: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 977: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 977: Rewards given")
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

return quest_977