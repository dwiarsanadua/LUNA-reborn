--[[
  Quest 390: [quest]Fluid of a strange plant
  Level Required: 56
  NPC Start: 92, NPC Complete: 92
  Prerequisites: [389]
  Rewards: EXP=392139, Gold=16000
]]

local fsm = require('fsm_engine')

local quest_390 = fsm:new({
    id = 390,
    name = "[quest]Fluid of a strange plant",
    level_required = 56,
    npc_start = 92,
    npc_complete = 92,
    prerequisites = {389},

    states = {
        {
            name = "NOT_STARTED",
            on_enter = function(self)
                self:log("Quest 390: Awaiting acceptance")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "IN_PROGRESS",
                    action = function(self)
                        self:log("Quest 390: Accepted")
                    end
                },
            },
        },

        {
            name = "IN_PROGRESS",
            on_enter = function(self)
                self:log("Quest 390: In progress")
            end,
            transitions = {
                {
                    trigger = "kill",
                    target_id = 254,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 390: Kill objective met")
                    end
                },
                {
                    trigger = "kill",
                    target_id = 255,
                    count = 2,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 390: Kill objective met")
                    end
                },
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "COMPLETE",
                    action = function(self)
                        self:log("Quest 390: NPC talk objective met")
                    end
                },
            },
        },

        {
            name = "COMPLETE",
            on_enter = function(self)
                self:log("Quest 390: All objectives done, turn in")
            end,
            transitions = {
                {
                    trigger = "npc_talk",
                    npc_id = 92,
                    target = "REWARDED",
                    action = function(self)
                        self:log("Quest 390: Completed!")
                    end
                },
            },
        },

        {
            name = "REWARDED",
            on_enter = function(self)
                self:log("Quest 390: Rewards given")
                self:give_rewards({
                    exp = 392139,
                    gold = 16000,
                    items = {
                        { item_id = 295, count = 10 },
                        { item_id = 296, count = 10 },
                    },
                })
            end,
            transitions = {},
        },
    },
})

return quest_390