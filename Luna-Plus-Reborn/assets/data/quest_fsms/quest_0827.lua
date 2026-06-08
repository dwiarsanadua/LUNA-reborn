--[[
  Quest 827: New Sample
  Level Required: 76
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [825]
  Rewards: EXP=1995585, Gold=61514
]]

local fsm = require('fsm_engine')

local quest_827 = fsm:new({
    id = 827,
    name = "New Sample",
    level_required = 76,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {825},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 827: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 827: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 827: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 260,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 827: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 827: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 827: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 827: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 827: Rewards given")
                self:give_rewards({
                    exp = 1995585,
                    gold = 61514,
                    items = {
                        { item_id = 389, count = 30 },
                        { item_id = 21000014, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_827