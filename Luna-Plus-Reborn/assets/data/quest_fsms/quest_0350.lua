--[[
  Quest 350: [B Class] Succubus Elimination
  Level Required: 60
  NPC Start: 83, NPC Complete: 83
  Prerequisites: [349]
  Rewards: EXP=729940
]]

local fsm = require('fsm_engine')

local quest_350 = fsm:new({
    id = 350,
    name = "[B Class] Succubus Elimination",
    level_required = 60,
    npc_start = 83,
    npc_complete = 83,
    prerequisites = {349},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 350: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 350: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 350: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 35,
                    count = 1,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 350: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 350: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 350: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 83,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 350: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 350: Rewards given")
                self:give_rewards({
                    exp = 729940,
                    items = {
                        { item_id = 256, count = 1 },
                        { item_id = 256, count = 50 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_350