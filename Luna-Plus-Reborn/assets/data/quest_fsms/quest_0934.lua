--[[
  Quest 934: His Voice 2
  Level Required: 103
  NPC Start: 537, NPC Complete: 537
  Prerequisites: [932]
  Rewards: EXP=11860806, Gold=81769
]]

local fsm = require('fsm_engine')

local quest_934 = fsm:new({
    id = 934,
    name = "His Voice 2",
    level_required = 103,
    npc_start = 537,
    npc_complete = 537,
    prerequisites = {932},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 934: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 934: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 934: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 297,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 934: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 298,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 934: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 299,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 934: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 934: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 934: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 537,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 934: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 934: Rewards given")
                self:give_rewards({
                    exp = 11860806,
                    gold = 81769,
                    items = {
                        { item_id = 21000282, count = 5 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_934