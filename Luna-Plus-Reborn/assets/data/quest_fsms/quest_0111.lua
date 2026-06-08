--[[
  Quest 111: Luscious Locks
  Level Required: 20
  NPC Start: 18, NPC Complete: 60
  Prerequisites: [96]
  Rewards: EXP=3733, Gold=825
]]

local fsm = require('fsm_engine')

local quest_111 = fsm:new({
    id = 111,
    name = "Luscious Locks",
    level_required = 20,
    npc_start = 18,
    npc_complete = 60,
    prerequisites = {96},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 111: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 60,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 111: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 111: In progress")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 60,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 111: NPC talk objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 18,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 111: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 111: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 60,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 111: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 111: Rewards given")
                self:give_rewards({
                    exp = 3733,
                    gold = 825,
                    items = {
                        { item_id = 39, count = 1 },
                        { item_id = 39, count = 1 },
                        { item_id = 21000008, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_111