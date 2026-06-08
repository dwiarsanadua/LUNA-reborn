--[[
  Quest 983: [Dungeon Quest] The Scary Temple of Greed (Intermediate)
  Level Required: 90
  NPC Start: 64, NPC Complete: 64
  Rewards: EXP=5222395, Gold=61188
]]

local fsm = require('fsm_engine')

local quest_983 = fsm:new({
    id = 983,
    name = "[Dungeon Quest] The Scary Temple of Greed (Intermediate)",
    level_required = 90,
    npc_start = 64,
    npc_complete = 64,
    prerequisites = {},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 983: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 983: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 983: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 646,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 983: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 647,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 983: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 648,
                    count = 50,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 983: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 983: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 983: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 64,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 983: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 983: Rewards given")
                self:give_rewards({
                    exp = 5222395,
                    gold = 61188,
                })
            end,
            transitions = {},
        },
    },
})

return quest_983