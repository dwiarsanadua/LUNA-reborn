--[[
  Quest 833: Evidence of Replication 1
  Level Required: 78
  NPC Start: 97, NPC Complete: 97
  Prerequisites: [832]
  Rewards: EXP=2435484, Gold=61514
]]

local fsm = require('fsm_engine')

local quest_833 = fsm:new({
    id = 833,
    name = "Evidence of Replication 1",
    level_required = 78,
    npc_start = 97,
    npc_complete = 97,
    prerequisites = {832},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 833: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 833: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 833: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 73,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 833: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 833: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 833: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 97,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 833: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 833: Rewards given")
                self:give_rewards({
                    exp = 2435484,
                    gold = 61514,
                    items = {
                        { item_id = 390, count = 1 },
                        { item_id = 390, count = 40 },
                        { item_id = 30000686, count = 1 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_833